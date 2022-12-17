import boto3
import time
import urllib
import json

AWS_DEFAULT_REGION = "us-east-1"
AWS_ACCESS_KEY_ID ="XXX"
AWS_SECRET_ACCESS_KEY = "XXXXXX"

#transcribe_client = boto3.client('transcribe')
# connect by using credentias
transcribe_client = boto3.client('transcribe', aws_access_key_id=AWS_ACCESS_KEY_ID,
                                                    aws_secret_access_key=AWS_SECRET_ACCESS_KEY,
                                                    region_name=AWS_DEFAULT_REGION)
JOB_NAME = "Example-job"


get_last_modified = lambda obj: int(obj['LastModified'].strftime('%s'))

# Obtener el nombre/path del ultimo archivo subido al bucket.
#s3 = boto3.client('s3')
#objs = s3.list_objects_v2(Bucket='max9814audiofiles')['Contents']
#last_added = [obj['Key'] for obj in sorted(objs, key=get_last_modified)][-1]


# Función para obtener el nombre/path del ultimo archivo subido al bucket.
def getLastFilePath():
    s3 = boto3.client('s3', aws_access_key_id=AWS_ACCESS_KEY_ID,
                                                    aws_secret_access_key=AWS_SECRET_ACCESS_KEY,
                                                    region_name=AWS_DEFAULT_REGION)
    
    get_last_modified = lambda obj: int(obj['LastModified'].strftime('%s'))
    objs = s3.list_objects_v2(Bucket='max9814audiofiles')['Contents']
    
    last_added = ""
    if len(objs) > 0:
        print('Se encontraron objetos en el bucket')
        # Obtener ultimo
        last_added = [obj['Key'] for obj in sorted(objs, key=get_last_modified)][-1]
        print(last_added)
    else:
        print("No hay objetos en el bucket")  

    return last_added


def transcribe_file(job_name, file_uri, transcribe_client):
    transcribe_client.start_transcription_job(
        TranscriptionJobName=job_name,
        Media={'MediaFileUri': file_uri},
        MediaFormat='wav',
        LanguageCode='es-ES' #english: en-US
    )

    max_tries = 60
    while max_tries > 0:
        max_tries -= 1
        job = transcribe_client.get_transcription_job(TranscriptionJobName=job_name)
        job_status = job['TranscriptionJob']['TranscriptionJobStatus']
        if job_status in ['COMPLETED', 'FAILED']:
            print(f"Job {job_name} is {job_status}.")
            if job_status == 'COMPLETED':
                response = urllib.request.urlopen(job['TranscriptionJob']['Transcript']['TranscriptFileUri'])
                data = json.loads(response.read())
                text = data['results']['transcripts'][0]['transcript']
                print("========== below is output of speech-to-text ========================")
                print(text)
                print("=====================================================================")
            break
        else:
            print(f"Waiting for {job_name}. Current status is {job_status}.")
        time.sleep(10)


def main():
    file_uri = 's3://max9814audiofiles/' + getLastFilePath()
    # Delete the job is already exists to prevent duplicate jobs problem error:
    # Obtiene una lista de trabajos de transcripción existentes
    response = transcribe_client.list_transcription_jobs()
    jobs = response['TranscriptionJobSummaries']
    jobs = filter(lambda x: x['TranscriptionJobName'] == JOB_NAME, jobs)
    jobs = list(jobs)
    if len(jobs) > 0:
        # El trabajo existe
        print(f"El trabajo {JOB_NAME} ya existe.")
        # Eliminarlo para que se pueda ejecutar la funcion transcribe_file. Sin embargo pude haber creado jobs con nombres unicos y evitar esta operacion de eliminacion.
        try:
            transcribe_client.delete_transcription_job(TranscriptionJobName=JOB_NAME)
        except ClientError as e:
            raise Exception( "boto3 client error el job no existe: " + e.__str__())
        except Exception as e:
            raise Exception( "Unexpected error eliminando job: " + e.__str__())
    else:
        # El trabajo no existe
        print(f"El trabajo {JOB_NAME} no existe. No hacer nada")

    # Transcribir
    transcribe_file(JOB_NAME, file_uri, transcribe_client)


if __name__ == '__main__':
    main()
